/*
 * YangAudioEncoderOpus.h
 *
 *  Created on: 2019年9月29日
 *      Author: yang
 */

#ifndef YANGENCODER_SRC_YANGAUDIOENCODEROPUS_H_
#define YANGENCODER_SRC_YANGAUDIOENCODEROPUS_H_
#include "opus/opus.h"
#include "yangutil/sys/YangLoadLib.h"
#include "yangencoder/YangAudioEncoder.h"

class YangAudioEncoderOpus: public YangAudioEncoder {
public:
	YangAudioEncoderOpus();
	virtual ~YangAudioEncoderOpus();
	void init(YangAudioInfo *pap);
	int32_t encoder(YangFrame* pframe,YangEncoderCallback* pcallback);

private:
		int32_t ret;
		int32_t m_frameSize;
		int32_t m_frameShortSize;
		OpusEncoder *m_encoder;
		YangLoadLib m_lib;

		uint8_t *m_cbits; //[YangSpeexBufferSize];
		uint8_t *m_input;
		short *m_in;
		short *m_input1; //[m_frameSize];


		void encoder(uint8_t *p_buf);
		void encoder_mono(uint8_t *p_buf);

		void closeEnc();
		void loadLib();
		void unloadLib();
		OpusEncoder *(*yang_opus_encoder_create)(opus_int32 Fs, int32_t channels, int32_t application,  int32_t *error);
		int32_t (*yang_opus_encoder_init)(OpusEncoder *st, opus_int32 Fs, int32_t channels, int32_t application) ;
		opus_int32 (*yang_opus_encode)(OpusEncoder *st,const opus_int16 *pcm, int32_t frame_size, uint8_t *data,
		    opus_int32 max_data_bytes);
		int32_t (*yang_opus_encoder_ctl)(OpusEncoder *st, int32_t request, ...);
		void (*yang_opus_encoder_destroy)(OpusEncoder *st);
		const char *(*yang_opus_strerror)(int32_t error);
};

#endif /* YANGENCODER_SRC_YANGAUDIOENCODEROPUS_H_ */
