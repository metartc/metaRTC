//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef YANGDECODER_SRC_YANGAUDIODECODERSPEEX_H_
#define YANGDECODER_SRC_YANGAUDIODECODERSPEEX_H_
#include <yangutil/buffer/YangAudioEncoderBuffer.h>
#include <yangdecoder/YangAudioDecoder.h>
#include <yangutil/sys/YangLoadLib.h>
#include <speex/speex.h>

class YangAudioDecoderSpeex  :public YangAudioDecoder{
public:
	YangAudioDecoderSpeex(YangAudioParam  *pcontext);
	virtual ~YangAudioDecoderSpeex();
	void init();
	int32_t decode(YangFrame* pframe,YangDecoderCallback* pcallback);
private:
	void closedec();
	int32_t isConvert;
	SpeexBits *m_bits;

	YangLoadLib m_lib;
	void initSpeexPara();
	int32_t m_quality;
	int32_t ret;
	void* m_state;

	int32_t m_channel;
	short *m_out;

	void loadLib();
	void unloadLib();
	const SpeexMode * (*yang_speex_lib_get_mode) (int32_t mode);
	void *(*yang_speex_decoder_init)(const SpeexMode *mode);
	void (*yang_speex_bits_init)(SpeexBits *bits);
	void (*yang_speex_bits_reset)(SpeexBits *bits);
	int32_t (*yang_speex_bits_read_from)(SpeexBits *bits, const char *bytes, int32_t len);
	int32_t (*yang_speex_decode_int)(void *state, SpeexBits *bits, spx_int16_t *out);
	void (*yang_speex_decoder_destroy)(void *state);
	void (*yang_speex_bits_destroy)(SpeexBits *bits);
	int32_t (*yang_speex_decoder_ctl)(void *state, int32_t request, void *ptr);
};

#endif /* YANGDECODER_SRC_YANGAUDIODECODERSPEEX_H_ */
