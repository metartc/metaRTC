//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef YANGENCODER_SRC_YANGAUDIOENCODEROPUS_H_
#define YANGENCODER_SRC_YANGAUDIOENCODEROPUS_H_
#include <yangencoder/YangAudioEncoder.h>
#include <yangutil/sys/YangLoadLib.h>
#include <opus/opus.h>


#if Yang_OS_ANDROID
#define Yang_Opus_So 0
#else
#define Yang_Opus_So 0
#endif
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


		uint8_t *m_cbits;
		uint8_t *m_input;
		short *m_in;
		short *m_input1;


		void encoder(uint8_t *p_buf);
		void encoder_mono(uint8_t *p_buf);

		void closeEnc();
#if Yang_Opus_So
		YangLoadLib m_lib;
		void loadLib();
		void unloadLib();
		OpusEncoder *(*yang_opus_encoder_create)(opus_int32 Fs, int32_t channels, int32_t application,  int32_t *error);
		int32_t (*yang_opus_encoder_init)(OpusEncoder *st, opus_int32 Fs, int32_t channels, int32_t application) ;
		opus_int32 (*yang_opus_encode)(OpusEncoder *st,const opus_int16 *pcm, int32_t frame_size, uint8_t *data,
		    opus_int32 max_data_bytes);
		int32_t (*yang_opus_encoder_ctl)(OpusEncoder *st, int32_t request, ...);
		void (*yang_opus_encoder_destroy)(OpusEncoder *st);
		const char *(*yang_opus_strerror)(int32_t error);

#else
#define yang_opus_encoder_create opus_encoder_create
	#define yang_opus_encoder_init opus_encoder_init
	#define yang_opus_encode opus_encode
	#define yang_opus_encoder_ctl opus_encoder_ctl
	#define yang_opus_encoder_destroy opus_encoder_destroy
	#define yang_opus_strerror opus_strerror
#endif
};

#endif /* YANGENCODER_SRC_YANGAUDIOENCODEROPUS_H_ */
