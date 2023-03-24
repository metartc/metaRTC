//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef YANGENCODER_SRC_YANGAUDIOENCODERSPEEX_H_
#define YANGENCODER_SRC_YANGAUDIOENCODERSPEEX_H_

#include <yangencoder/YangAudioEncoder.h>
#include <yangencoder/YangAudioEncoder.h>
#include <yangutil/sys/YangLoadLib.h>
#include <speex/speex.h>



class YangAudioEncoderSpeex: public YangAudioEncoder {
public:
	YangAudioEncoderSpeex();
	virtual ~YangAudioEncoderSpeex();
	void init(YangAudioInfo *pap);
	int32_t encoder(YangFrame* pframe,YangEncoderCallback* pcallback);

private:
	int32_t isConvert;
	SpeexBits m_bits;

	YangLoadLib m_lib;
	void initSpeexPara();
	int32_t m_quality;
	int32_t ret;
	spx_int32_t skip_group_delay;
	void *m_state;
	char *m_cbits;
	int32_t m_bufLen;
	char *temp;
	float *m_input;
	short *m_input1;
	short *m_in;

	void encoder(char *p_buf);
	void encoder_mono(char *p_buf);
	void closeSpeex();
	void saveWave();
	void loadLib();
	void unloadLib();
	const SpeexMode * (*yang_speex_lib_get_mode) (int32_t mode);
	void *(*yang_speex_encoder_init)(const SpeexMode *mode);
	void (*yang_speex_bits_init)(SpeexBits *bits);
	void (*yang_speex_bits_reset)(SpeexBits *bits);
	int32_t (*yang_speex_bits_write)(SpeexBits *bits, char *bytes, int32_t max_len);
	int32_t (*yang_speex_encode)(void *state, float *in, SpeexBits *bits);
	void (*yang_speex_encoder_destroy)(void *state);
	void (*yang_speex_bits_destroy)(SpeexBits *bits);
	int32_t (*yang_speex_encoder_ctl)(void *state, int32_t request, void *ptr);
};

#endif /* YANGENCODER_SRC_YANGAUDIOENCODERSPEEX_H_ */
