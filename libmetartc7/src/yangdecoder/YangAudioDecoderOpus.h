//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YANGDECODER_SRC_YANGAUDIODECODEROPUS_H_
#define YANGDECODER_SRC_YANGAUDIODECODEROPUS_H_
#include <yangdecoder/YangAudioDecoder.h>
#include <yangutil/sys/YangLoadLib.h>
#include <opus/opus.h>

#if Yang_OS_ANDROID
#define Yang_Opus_So 0
#else
#define Yang_Opus_So 0
#endif
class YangAudioDecoderOpus:public YangAudioDecoder {
public:
	YangAudioDecoderOpus(YangAudioParam *pcontext);
	virtual ~YangAudioDecoderOpus();
	void init();
	int32_t decode(YangFrame* pframe,YangDecoderCallback* pcallback);
	private:
	int32_t yang_opus_insertData(int32_t ret,YangFrame* pframe,YangDecoderCallback* pcallback);
		void closedec();
		int32_t isConvert;
		OpusDecoder *m_decoder;
		int64_t m_prePts;
		int32_t m_frameTime;
		int32_t m_fec;
		int32_t m_isMono;
		int32_t ret;
		int32_t m_sample;
		int32_t m_channel;
		short *m_out;
		uint8_t *m_output;
#if Yang_Opus_So
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
#else
#define yang_opus_decoder_create opus_decoder_create
#define yang_opus_decoder_init opus_decoder_init
#define yang_opus_decode opus_decode
#define yang_opus_decoder_ctl opus_decoder_ctl
#define yang_opus_decoder_destroy opus_decoder_destroy
#define yang_opus_strerror opus_strerror
#endif
};

#endif /* YANGDECODER_SRC_YANGAUDIODECODEROPUS_H_ */
